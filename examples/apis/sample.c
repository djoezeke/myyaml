
#include <myyaml/myyaml.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int help = 0;
    int canonical = 0;
    int unicode = 0;
    int k;
    int done = 0;

    YamlParser parser;
    YamlEmitter emitter;
    YamlEvent event;
    FILE *in, *out;
    in = fopen("fruit.yaml", "rb");
    out = fopen("fruit1.yaml", "wb");

    /* Clear the objects. */
    memset(&parser, 0, sizeof(parser));
    memset(&emitter, 0, sizeof(emitter));
    memset(&event, 0, sizeof(event));

    /* Analyze command line options. */
    for (k = 1; k < argc; k++) {
        if (strcmp(argv[k], "-h") == 0 || strcmp(argv[k], "--help") == 0) {
            help = 1;
        }

        else if (strcmp(argv[k], "-c") == 0 || strcmp(argv[k], "--canonical") == 0) {
            canonical = 1;
        }

        else if (strcmp(argv[k], "-u") == 0 || strcmp(argv[k], "--unicode") == 0) {
            unicode = 1;
        }

        else {
            fprintf(stderr,
                    "Unrecognized option: %s\n"
                    "Try `%s --help` for more information.\n",
                    argv[k], argv[0]);
            return 1;
        }
    }

    /* Display the help string. */
    if (help) {
        printf(
            "%s [--canonical] [--unicode] <input >output\n"
            "or\n%s -h | --help\nReformat a YAML stream\n\nOptions:\n"
            "-h, --help\t\tdisplay this help and exit\n"
            "-c, --canonical\t\toutput in the canonical YAML format\n"
            "-u, --unicode\t\toutput unescaped non-ASCII characters\n",
            argv[0], argv[0]);
        return 0;
    }

    /* Initialize the parser and emitter objects. */
    if (!yaml_parser_initialize(&parser)) goto parser_error;
    if (!yaml_emitter_initialize(&emitter)) goto emitter_error;

    /* Set the parser parameters. */
    yaml_parser_set_input_file(&parser, in);

    /* Set the emitter parameters. */
    yaml_emitter_set_output_file(&emitter, out);

    yaml_emitter_set_canonical(&emitter, canonical);
    yaml_emitter_set_unicode(&emitter, unicode);

    /* The main loop. */
    while (!done) {
        /* Get the next event. */
        if (!yaml_parser_parse(&parser, &event)) goto parser_error;

        /* Check if this is the stream end. */
        if (event.type == YAML_STREAM_END_EVENT) {
            done = 1;
        }

        /* Emit the event. */
        if (!yaml_emitter_emit(&emitter, &event)) goto emitter_error;
    }

    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 0;

parser_error:

    /* Display a parser error message. */
    switch (parser.error) {
        case YAML_MEMORY_ERROR:
            fprintf(stderr, "Memory error: Not enough memory for parsing\n");
            break;

        case YAML_READER_ERROR:
            if (parser.problem_value != -1) {
                fprintf(stderr, "Reader error: %s: #%X at %ld\n", parser.problem, parser.problem_value, (long)parser.problem_offset);
            } else {
                fprintf(stderr, "Reader error: %s at %ld\n", parser.problem, (long)parser.problem_offset);
            }
            break;

        case YAML_SCANNER_ERROR:
            if (parser.context) {
                fprintf(stderr,
                        "Scanner error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n",
                        parser.context, (int)parser.context_mark.line + 1, (int)parser.context_mark.column + 1, parser.problem,
                        (int)parser.problem_mark.line + 1, (int)parser.problem_mark.column + 1);
            } else {
                fprintf(stderr, "Scanner error: %s at line %d, column %d\n", parser.problem, (int)parser.problem_mark.line + 1,
                        (int)parser.problem_mark.column + 1);
            }
            break;

        case YAML_PARSER_ERROR:
            if (parser.context) {
                fprintf(stderr,
                        "Parser error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n",
                        parser.context, (int)parser.context_mark.line + 1, (int)parser.context_mark.column + 1, parser.problem,
                        (int)parser.problem_mark.line + 1, (int)parser.problem_mark.column + 1);
            } else {
                fprintf(stderr, "Parser error: %s at line %d, column %d\n", parser.problem, (int)parser.problem_mark.line + 1,
                        (int)parser.problem_mark.column + 1);
            }
            break;

        default:
            /* Couldn't happen. */
            fprintf(stderr, "Internal error\n");
            break;
    }

    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 1;

emitter_error:

    /* Display an emitter error message. */
    switch (emitter.error) {
        case YAML_MEMORY_ERROR:
            fprintf(stderr, "Memory error: Not enough memory for emitting\n");
            break;

        case YAML_WRITER_ERROR:
            fprintf(stderr, "Writer error: %s\n", emitter.problem);
            break;

        case YAML_EMITTER_ERROR:
            fprintf(stderr, "Emitter error: %s\n", emitter.problem);
            break;

        default:
            /* Couldn't happen. */
            fprintf(stderr, "Internal error\n");
            break;
    }

    yaml_parser_delete(&parser);
    yaml_emitter_delete(&emitter);

    return 1;
}

/**
 * LICENSE: Public Domain (www.unlicense.org)
 *
 * Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 * This is free and unencumbered software released into the public domain.
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
 * software, either in source code form or as a compiled binary, for any purpose,
 * commercial or non-commercial, and by any means.
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the public
 * domain. We make this dedication for the benefit of the public at large and to
 * the detriment of our heirs and successors. We intend this dedication to be an
 * overt act of relinquishment in perpetuity of all present and future rights to
 * this software under copyright law.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */