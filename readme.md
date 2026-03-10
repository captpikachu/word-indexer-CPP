# Memory Efficient File Indexer

**Author:** Yash Bhardwaj

------------------------------------------------------------------------

## Overview

This project implements a **memory-efficient text file indexing system**
in C++.\
The program processes large text files using a **fixed-size buffer**
instead of loading the entire file into memory.

While reading the file chunk-by-chunk, the program tokenizes words and
stores their frequency in a map for each version of the file.

The system supports queries to: - find the frequency of a word - compare
frequencies across two versions - display the most frequent words in a
file

------------------------------------------------------------------------

## Features

The program supports three query types:

### Word Frequency

Returns the number of times a word appears in a file version.

### Difference Query

Computes the difference in frequency of a word between two versions.

### Top-K Frequent Words

Displays the most frequent words in a version.

------------------------------------------------------------------------

## Compilation

Compile the program using **g++ with C++17**:

    g++ -std=c++17 -O3 -o analyzer main.cpp

------------------------------------------------------------------------

## Usage

### Word Frequency Query

    ./analyzer --file file.txt --version v1 --buffer 512 --query word --word example

### Difference Query

    ./analyzer --file1 file1.txt --version1 v1            --file2 file2.txt --version2 v2            --buffer 512 --query diff --word example

### Top-K Frequent Words

    ./analyzer --file file.txt --version v1 --buffer 512 --query top --top 10

------------------------------------------------------------------------

## Command Line Arguments

  Argument                     Description
  ---------------------------- ------------------------------------
  `--file`                     Input file
  `--file1`, `--file2`         Files used for comparison
  `--version`                  Version name
  `--version1`, `--version2`   Version names for diff query
  `--buffer`                   Buffer size in KB (256--1024)
  `--query`                    Query type (`word`, `diff`, `top`)
  `--word`                     Target word
  `--top`                      Number of frequent words

------------------------------------------------------------------------

## Design Summary

The program uses an object-oriented design with inheritance and
templates.

Class hierarchy:

    base → versionIndexing → buffer → tokenization

-   **base** : Abstract class defining the tokenize interface.
-   **versionIndexing** : Stores version information and word
    frequencies.
-   **buffer** : Reads the file using a fixed-size buffer.
-   **tokenization** : Implements tokenization and updates word counts.

Queries are implemented using template-based classes: - `wordQuery` -
`diffQuery` - `topKQuery`

Each query implements an `execute()` method.

------------------------------------------------------------------------

## File Processing

Files are processed using a **fixed-size buffer** allocated as:

    vector<char> internal_buf(BUFFER_SIZE);

The file is read chunk-by-chunk using:

    file.read(internal_buf.data(), BUFFER_SIZE);

Characters are examined one by one to detect words and delimiters.\
Words are converted to lowercase and stored in the frequency map.

This approach allows the program to process **very large files
efficiently** while keeping memory usage low.

------------------------------------------------------------------------

## Assumptions

-   Words consist only of **alphanumeric characters**
-   Word comparison is **case-insensitive**
-   Buffer size must be between **256KB and 1024KB**
-   Input files are **plain text files**
