![](DSS.png)

# DSS - Dead Simple Slides

Dead Simple Slides (DSS) is a text/terminal based slide presentation tool that reads .txt files for for formatted slides. These slides are then displayed to the terminal.

DSS is in the very early stages of development. At this moment it can display the slides in the sample.txt files profided, but it can't do much else. 

### Planned Features

DSS's main goal is to display text slides in their raw format, so outside of this functionality there probably won't be many other features to note.

DSS can be navigated using `K`/`J` for prev/next slide, `g`/`G` for first/last slide, and `Q` for quit

A few features that are planned include:
+ simple color formatting
+ url detection and selection with H/L
+ bookmarking and jumping to bookmarks 

### Installation

cd into the directory and run the following command:

`gcc -I ./include src/main.c -o dss -lncurses`

or simply run the Makefile:

`make`

### Usage

With the latest version of DSS you can simply run the program with a text file argument:

`./dss sample.txt`

Note that you will need to include `title`, `areaX`, `areaY`, and `slides` variables inside your txt file. See the sample files for more details.

### Limitations

The current version of DSS supports slides consisting of up to 5000 characters. This translates to about 70x70. The sample files are 50x15 and 100x25.

### Contributing

Contribution to the project is welcome and encouraged. If you'd like to contribute please follow these guidelines:
1. If you are working on an issue, comment on the issue and reference it when making a merge request.
2. If you can provide test resources for your fix/feature please do.
