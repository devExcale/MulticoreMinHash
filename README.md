# Multicore MinHash

This project contains two C implementations of the MinHash algorithm that use parallelism to improve performances.
The two implementations use MPI one and OpenMP the other to parallelize computation.

The algorithm's output is a csv file that contains the indices of the documents that matched
and the similarity score between them.

## Code structure

The code can be found inside the `src` folder:
the `MPI` and `OMP` folders contain the implementations of the algorithm using MPI and OpenMP respectively.
Moreover, python scripts with various purposes can be found in the `src` folder.

The whole project can be built using the `Makefile` in the root folder.
During compilation, the `whichmp` variable must be used to specify which implementation to compile.
The possible values are `MPI` and `OMP`.

## Running options

- `docs`: the number of documents to use when running the program
- `offset`: the number of documents to skip when running the program
- `shingle`: the number of words to use for each shingle
- `signature`: the number of hash functions to use for each signature
- `bandrows`: the number of rows to use for each band
- `seed`: the seed to use for the hash functions
- `threshold`: the similarity threshold to use when filtering the results

## Makefile rules

The `Makefile` contains the following rules:

- `all`: compiles the project using the specified implementation
- `clean`: removes all the compiled files
- `run`: runs the compiled program
- `debug`: runs the compiled program with the `gdb` debugger
- `time`: runs the program with the `time` command
- `report`: runs the program multiple times with increasing number of processes
  and saves the execution times in a csv file
- `report-check`: checks that the csv outputs of the multiple runs by `report` are consistent
- `extract-medpub`: extracts the MedPub dataset from kaggle's csv file

## Make options

The makefile contains ready-to-use configurations that can be used to test the algorithm out-of-the-box.

- `whichmp`: the implementation to use when compiling or running the program
- `processes`: the number of processes to use when running once,
  or the maximum number of processes to use when running multiple times
- `dataset`: the dataset to use when running the program (see [below](#datasets) for more information)
- `repeat`: the number of times to run the program with the same number of processes when using the `report` rule

> **Example:** the command `make report whichmp=OMP processes=12 repeat=3 dataset=medical` will run the OMP implementation on
the `medical` dataset from 1 to 12 processes, 3 times for each number of processes, for a total of 36 executions.

## Datasets

The datasets we used to test the performance of the algorithms are downloadable from the Kaggle platform.

| Name                                         | Execution code |  # Docs | Link                                                                                          |
|----------------------------------------------|:--------------:|--------:|-----------------------------------------------------------------------------------------------|
| 2k clean medical articles (MedicalNewsToday) |   `medical`    |   1'989 | [link](https://www.kaggle.com/datasets/trikialaaa/2k-clean-medical-articles-medicalnewstoday) |
| 🌍 Environment News Dataset 📰               | `environment`  |  29'090 | [link](https://www.kaggle.com/datasets/beridzeg45/guardian-environment-related-news)          |
| PubMed Article Summarization Dataset         |    `medpub`    | 106'330 | [link](https://www.kaggle.com/datasets/thedevastator/pubmed-article-summarization-dataset)    |
