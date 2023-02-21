package uniqueHamGraphs;

import org.apache.commons.cli.*;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;

public class Main {

    private static Option createOption(String opt, boolean required, String desc, boolean hasArg) {
        return  Option.builder(opt)
                .required(required)
                .desc(desc)
                .hasArg(hasArg)
                .build();
    }


    public static void main(String[] args) {

        //args = new String[] {"-n", "18", "-minDeg", "3", "-girth", "5", "-show", "-all"};
//        args = new String[] {"-n", "5", "-all"};
        Options options = new Options();


        Option optN =           createOption("n", true, "number of vertices", true);
        Option optNoSymBreak =  createOption("noSB", false, "no symmetry breaking", false);
        Option optNearlyCubic = createOption("ncDS", false, "nearly cubic degree sequence index", true);
        Option optMinDeg =      createOption("minDeg", false, "minimal degree", true);
        Option optMaxDeg =      createOption("maxDeg", false, "maximal degree", true);
        Option optChords =      createOption("chords", false, "number of chords", true);
        Option optExtremal =    createOption("extremal", false, "with maximum number of edges", false);
        Option optMinchord =    createOption("shortestChord", false, "shortest chord length", true);
        Option optMaxchord =    createOption("longestChord", false, "longest chord length", true);
        Option optSolveAll =    createOption("all", false, "find all solutions", false);
        Option optGirth =       createOption("girth", false, "girth at least", true);
        Option optSplit =       createOption("split", false, "split", true);
        Option optSplitLevel =  createOption("splitLevel", false, "split level", true);
        Option optShow =        createOption("show", false, "show solutions", false);
        Option optReg =        createOption("reg", false, "d-regular graph", true);


        options.addOption(optN);
        options.addOption(optNoSymBreak);
        options.addOption(optNearlyCubic);
        options.addOption(optMinDeg);
        options.addOption(optMaxDeg);
        options.addOption(optChords);
        options.addOption(optExtremal);
        options.addOption(optMinchord);
        options.addOption(optMaxchord);
        options.addOption(optSolveAll);
        options.addOption(optGirth);
        options.addOption(optSplit);
        options.addOption(optSplitLevel);
        options.addOption(optShow);
        options.addOption(optReg);



        CommandLineParser parser = new DefaultParser();

        int n = 0;
        boolean sb = true;
        boolean extremal = false;
        int[] nearlyCubicDS = null;
        int dsIndex = -1;
        int minDeg = -1;
        int maxDeg = -1;
        int chords = -1;
        int shortest = 0;
        int longest = 0;
        boolean all = false;
        int girth = -1;
        int split = -1;
        boolean show = false;
        int splitLevel = -1;
        int deg = -1;

        try {
            CommandLine line = parser.parse( options, args );

            n = Integer.parseInt(line.getOptionValue("n"));

            if (line.hasOption("noSB")) {
                sb = false;
            }

            if (line.hasOption("ncDS")) {
                dsIndex = Integer.parseInt((line.getOptionValue("ncDS")));
                int k = dsIndex;
                for (int[] ds : Utils.nearlyCubicDS(n)) {
                    if (k == 0) {
                        nearlyCubicDS = ds;
                    }
                    k--;
                }
            }

            if (line.hasOption("minDeg")) {
                minDeg = Integer.parseInt((line.getOptionValue("minDeg")));
            }

            if (line.hasOption("maxDeg")) {
                maxDeg = Integer.parseInt((line.getOptionValue("maxDeg")));
            }

            if (line.hasOption("chords")) {
                chords =  Integer.parseInt((line.getOptionValue("chords")));
            }

            if (line.hasOption("extremal")) {
                extremal = true;
                chords = (n*n)/4 + 1 - n;
            }

            if(line.hasOption("all")) {
                all = true;
            }

            if (line.hasOption("shortestChord")) {
                shortest =  Integer.parseInt((line.getOptionValue("shortestChord")));
            }
            if (line.hasOption("longestChord")) {
                longest =  Integer.parseInt((line.getOptionValue("longestChord")));
            }

            if (line.hasOption("girth")) {
                girth = Integer.parseInt((line.getOptionValue("girth")));
            }

            if (line.hasOption("split")) {
                split = Integer.parseInt((line.getOptionValue("split")));
            }
            if (line.hasOption("splitLevel")) {
                splitLevel = Integer.parseInt((line.getOptionValue("splitLevel")));
            }

            if (line.hasOption("show")) {
                show = true;
            }
            if (line.hasOption("reg")) {
                deg = Integer.parseInt((line.getOptionValue("reg")));;
            }


        }
        catch( ParseException exp ) {
            HelpFormatter formatter = new HelpFormatter();
            formatter.printHelp( "gen1hcp", options );
            System.err.println( "Parsing failed.  Reason: " + exp.getMessage() );
        }

        String outFile = "uhg_" + n;
        UHGSearch gen = new UHGSearch(n);
        if (sb) {
            gen.symmetryBreaking();
        }
        else {
            outFile += "_nosym";
        }

        if (nearlyCubicDS != null) {
            gen.fixDegrees(nearlyCubicDS);
            outFile += "_nc_ds_" + dsIndex;
        }
        if (minDeg > 0) {
            gen.minDegree(minDeg);
            outFile += "_mindeg_" + minDeg;
        }

        if (maxDeg > -1) {
            gen.maxDegree(maxDeg);
            outFile += "_maxDeg_" + maxDeg;
        }

        if (extremal) {
            gen.numberOfChords(chords);
            outFile += "_extremal";
        }
        else if (chords > -1) {
            gen.numberOfChords(chords);
            outFile += "_chords_" + chords;
        }


        if (shortest > 1) {
            gen.shortestChord(shortest);
            outFile += "_shortest_chord_" + shortest;
        }

        if (longest > 1) {
            gen.longestChord(longest);
            outFile += "_longest_chord_" + longest;
        }

        if (girth > -1) {
            gen.girthAtLeast(girth);
            outFile += "_girth_" + girth;
        }
        if (split >-1) {
            if (splitLevel > -1) {
                gen.setSplitLevel(splitLevel);
                outFile += "_splitLevel_" + splitLevel;
            }
            gen.setCase(split);
            //outFile += "_case_" + split;
        }
        if (deg > -1) {
            gen.regular(deg);
            outFile +=  "_" + deg + "reg_";
        }

        if (show) {
            gen.showSolutions();
        }

        outFile += ".stats";
        PrintStream outFileStream = null;
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(outFile, true);
            fos.getChannel().lock();
            outFileStream = new PrintStream(fos);
            if (all) {
                gen.solveAll(outFileStream);
            }
            else {
                gen.solve(outFileStream);
            }
            //outFileStream.println("ham paths = " + PropNoHamCycleStartsWithEdge.sumPaths);
            //outFileStream.println("executions = " + PropNoHamCycleStartsWithEdge.executions);
            //System.out.println(PropNoHamCycleWithEdge.sumPaths);
            //System.out.println(PropNoHamCycleWithEdge.executions);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        finally {
            try {
                if (fos != null) fos.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }

}
