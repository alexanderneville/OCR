#ifndef detection_H
#define detection_H

#define CHAR_DIMENSIONS 32

typedef struct character_T {

    float * pixels;
    int x, y, w, h;

    int line, word;

} character;

typedef struct word_T {

    float * pixels;
    int x, y, w, h;

    character * character;
    int num_characters, max_characters;
    int line;

} word;

typedef struct line_T {

    float * pixels;
    int x, y, w, h;

    word * words;
    int num_words, max_words;

} line;

typedef struct document_T {

    float * pixels;
    int x, y, w, h;

    line * lines;
    int num_lines, max_lines;

} document;


document * new_document(float * pixels, int height, int width);
line * new_line(float * pixels, int x, int y, int w, int h);
word * new_word(float * pixels, int x, int y, int w, int h);
character * new_character(float * pixels, int x, int y, int w, int h);

#endif
