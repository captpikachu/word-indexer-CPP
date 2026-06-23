# Memory Efficient File Indexer

**Author:** Yash Bhardwaj

------------------------------------------------------------------------

## Overview

This program implements a **memory-efficient text file indexing system**
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

    g++ -std=c++17 -O3 -o analyzer Code.cpp -lpsapi

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
Words are converted to lowercase and stored in the frequency map the map size will increase a number of unique word increase and requiring more memory but still way less than full loading.

This approach allows the program to process **very large files
efficiently** while keeping memory usage low.

------------------------------------------------------------------------

## Assumptions

-   Words consist only of **alphanumeric characters**
-   Word comparison is **case-insensitive**
-   Buffer size must be between **256KB and 1024KB**
-   Input files are **plain text files**
-   Realistically an advanced english speaker knows about 20-25k words suppose he looks every 5th word which he doesn't know that makes about 30k unique words for any usage and thus realistically a file wouldn't have 70k unique words as tested but much lesser than that , for 30k words the memory reduction for 500mb file was 99%.

------------------------------------------------------------------------

## Benchmarking & Memory Optimization

To validate the efficiency of the chunked buffer approach, this system was benchmarked against a **Full Load** script that reads the entire file into memory before tokenization. 

* **Fair Comparison:** To ensure an apples-to-apples comparison, the naive full-load script uses the exact same `map<string, int>` tokenization and indexing logic.
* **Results:** For a 500 MB text file with 70k unique words randomly generated not necesssary meaningful, the full-load approach requires **>500 MB** of peak RAM. In contrast, this chunked Word Indexer processes the same file using only **~10-12 MB** of peak RAM (saving >97% of memory and upto 99% for small files with less unique words) while maintaining comparable processing speed.

*Note: While the current system only stores word frequency (`map<string, int>`), chunked file reading is still the standard for advanced search indexing. For general-purpose reading where tracking the exact location of a word is required, the data structure can simply be updated to `map<string, vector<int>>` to store byte offsets or line numbers without sacrificing any of the memory benefits.*
To run the script for complete file loading with similar map structure
**Compile:**

    g++ -std=c++17 -O3 -o complete_loading complete_loading.cpp -lpsapi

**Run:**

    ./complete_loading test_500mb.txt
