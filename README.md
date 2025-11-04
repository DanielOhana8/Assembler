# Assembler

A two-pass assembler written in C for a custom language.

## Overview
This program reads `.as` source files, processes them through a pre-assembler, first pass, and second pass, and produces the output files:
`.am`, `.ob`,`.ent` and `.ext`.

The assembler follows the specifications described in **AssemblerGuide.pdf**.

## Build
To compile the project (using the included `makefile`):

```bash
make
./assembler exemple1.as exemple2.as
