# DSS - Dead Simple Slides

Dead Simple Slides (DSS) is a text/terminal based slide presentation tool that reads .txt files for for formatted slides. These slides are then displayed to the terminal.

DSS is in the very early stages of development. At this moment it can display the slides in the sample.txt files profided, but it can't do much else. 

### Planned Features

DSS's main goal is to display text slides in their raw format, so outside of this functionality there probably won't be many other features to note.

DSS can be navigated using K/J for prev/next slide and Q for quit

A few features that are planned include:
+ simple color formatting
+ url detection and selection with H/L
+ bookmarking and jumping to bookmarks 

### Installation

cd into the src directory and run the following command:

`gcc -I path/to/DSS/include main.c -o dss -lncurses`

or simply run the Makefile:

`make`

### Usage

Assuming that you have compiled the main.c file as a simple a.out file and you are using the sample txt, you can do this:

`./dss -x 51 -y 16 -s 5 sample.txt`

Note that you can omit the x/y/s values. Defaults are 100/30/15.

### Contributing

Contribution to the project is welcome and encouraged. If you'd like to contribute please follow these guidelines:
1. If you are working on an issue, comment on the issue and reference it when making a merge request.
2. If you can provide test resources for your fix/feature please do.
