# MP3 Tag Reader & Editor

A C-based MP3 Tag Reader and Editor that reads and modifies ID3v2.3 metadata stored in an MP3 file.

## Project Overview

This project implements an MP3 metadata reader and editor using C programming.

The application reads the ID3v2.3 tag information from an MP3 file and displays important metadata such as:

- Title
- Artist
- Album
- Year
- Genre
- Composer

It also allows the user to modify selected tag values directly in the MP3 file.

## Features

### View MP3 Tags

The application can read and display ID3v2.3 metadata.

```bash
./a.out -v tamilsong.mp3
