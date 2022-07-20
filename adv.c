/* adventure game engine */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_VERBS 20
#define NUM_DIRS 10

struct object {
    char *name;
    char *title;
    char *at;
    char *verbs[MAX_VERBS];
    char *verbNames[MAX_VERBS];
    int numVerbs;
};

struct location {
    char *name;
    char *title;
    char *description;
    char *exits[NUM_DIRS];
};

struct object defobj;
struct location defloc;
struct location locations[50];
struct object objects[200];
int numLocations=0, numObjects=0;
char *currentLocation=0;
char *currentObject=0;
char *words[30];
int numWords=0;
int nextWordIndex;
const char *dirStrings[] = {
    "north", "east", "south", "west",
    "north-east", "south-east", "north-west", "south-west",
    "up", "down",
};
const char *dStrings[] = { "n","e","s","w","ne","se","nw","sw","u","d", };

void addVerb(struct object *o, char *verbName, char *verb) {
    int i;
    for(i = 0; i < o->numVerbs; i++)
        if(!strcmp(o->verbNames[i], verbName)) {
            free(o->verbNames[i]);
            free(o->verbs[i]);
            o->numVerbs--;
            o->verbNames[i] = o->verbNames[o->numVerbs];
            o->verbs[i] = o->verbs[o->numVerbs];
            break;
        }

    if(!strcmp(verb, "nil"))
        return;

    o->verbs[o->numVerbs] = malloc(strlen(verb)+1);
    strcpy(o->verbs[o->numVerbs], verb);
    o->verbNames[o->numVerbs] = malloc(strlen(verbName)+1);
    strcpy(o->verbNames[o->numVerbs], verbName);
    o->numVerbs++;
}

struct object *findObject(char *name) {
    int i;
    for(i = numObjects-1; i >= 0; i--)
        if(!strcmp(objects[i].name, name))
            return &objects[i];
    return 0;
}

struct location *findLocation(char *name) {
    int i;
    for(i = numLocations-1; i >= 0; i--)
        if(!strcmp(locations[i].name, name))
            return &locations[i];
    return 0;
}

char locationHas(char *lname, char *name) {
    int i;
    for(i = 0; i < numObjects; i++)
        if(!strcmp(objects[i].at, lname) && !strcmp(objects[i].name, name))
            return 1;
    return 0;
}

int locationObjectNum(char *lname) {
    int n, i;
    n = 0;
    for(i = 0; i < numObjects; i++)
        if(!strcmp(objects[i].at, lname)) n++;
    return n;
}

void eval(const char *filename, char *text);

void doVerb(struct object *o, char *verbName) {
    int i;
    for(i = 0; i < o->numVerbs; i++)
        if(!strcmp(o->verbNames[i], verbName)) {
            currentObject = o->name;
            eval(0, o->verbs[i]);
            return;
        }
    printf("You can't %s that!\n", verbName);
}

const char *boolStr(int c) {
    if(c) return "t";
    return "nil";
}

int getDIndex(char *d) {
    int i;
    switch(d[0]) {
    case 'n': i = 0; break;
    case 'e': i = 1; break;
    case 's': i = 2; break;
    case 'w': i = 3; break;
    case 'u': i = 8; break;
    case 'd': i = 9; break;
    }
    if(d[1] == 'w')
        i += 4;
    else if(d[1] == 'e')
        i += 6;
    return i;
}

int listItems(const char *pre, char *lname);

void eval(const char *filename, char *text) {
    char *symbols[100];
    int numSymbols = 0;
    FILE *fp;
    char c;
    char *tp = text;
    char s[2000];
    int len = 0;
    int i, j, d;
    int sdepth, depth=0;
    char sep;
    const char *delim = "() \t\n'\";";
    char *a;
    char comma = 0, quote = 0, comment = 0;
    struct object *o;
    struct location *l;
    const char *r;

    if(filename) {
        fp = fopen(filename, "r");
        assert(fp);
    }

    for(c=1; c != 0 && c != EOF;) {
        if(filename)
            c = fgetc(fp);
        else
            c = *(tp++);

        if(comment) {
            if(c == '\n') comment = 0;
            continue;
        }

        if(comma) {
            s[len++] = c;
            if(c == '(') sdepth++;
            if(c == ')')
                if(--sdepth == 0) {
                    s[len] = 0;
                    comma = 0;
                    a = malloc(len+1);
                    strcpy(a, s);
                    symbols[numSymbols++] = a;
                    len = 0;
                }
            continue;
        }

        if(quote) {
            if(c == '"')
                quote = 0;
            else
                s[len++] = c;
            continue;
        }

        sep = 0;
        for(i = 0; delim[i] && !sep; i++)
            if(c == delim[i]) sep = 1;

        if(sep) {
            if(len) {
                s[len] = 0;
                a = malloc(strlen(s)+1);
                strcpy(a, s);
                symbols[numSymbols++] = a;
                len = 0;
            }
        }
        else {
            s[len++] = c;
            continue;
        }

        if(c == ';') {
            comment = 1;
            continue;
        }

        if(c == '\'') {
            comma = 1;
            sdepth = 0;
            continue;
        }

        if(c == '"') {
            quote = 1;
            continue;
        }

        if(c == '(') {
            depth++;
            a = malloc(2);
            a[0] = '(';
            a[1] = 0;
            symbols[numSymbols++] = a;
            continue;
        }

        if(c == ')') {
            for(i = numSymbols-1; strcmp(symbols[i], "("); i--);
            i++;
            a = symbols[i];
            r = 0;

            if(!strcmp(a, "if")) {
                if(strcmp(symbols[i+1], "nil"))
                    eval(0, symbols[i+2]);
                else if(i+3 < numSymbols)
                    eval(0, symbols[i+3]);
            }
            else if(!strcmp(a, "default"))
                addVerb(&defobj, symbols[i+1], symbols[i+2]);
            else if(!strcmp(a, "how"))
                addVerb(findObject(symbols[i+1]), symbols[i+2], symbols[i+3]);
            else if(!strcmp(a, "x"))
                r = currentObject;
            else if(!strcmp(a, "="))
                r = boolStr(!strcmp(symbols[i+1], symbols[i+2]));
            else if(!strcmp(a, ">"))
                r = boolStr(atoi(symbols[i+1]) > atoi(symbols[i+2]));
            else if(!strcmp(a, "<"))
                r = boolStr(atoi(symbols[i+1]) < atoi(symbols[i+2]));
            else if(!strcmp(a, ">="))
                r = boolStr(atoi(symbols[i+1]) >= atoi(symbols[i+2]));
            else if(!strcmp(a, "<="))
                r = boolStr(atoi(symbols[i+1]) <= atoi(symbols[i+2]));
            else if(!strcmp(a, "has"))
                r = boolStr(locationHas(symbols[i+1], symbols[i+2]));
            else if(!strcmp(a, "here"))
                r = currentLocation;
            else if(!strcmp(a, "goto")) {
                if(currentLocation) free(currentLocation);
                currentLocation = malloc(strlen(symbols[i+1])+1);
                strcpy(currentLocation, symbols[i+1]);
                l = findLocation(currentLocation);
                printf("[%s]\n", l->title);
            }
            else if(!strcmp(a, "list-objects"))
                listItems(symbols[i+2], symbols[i+1]);
            else if(!strcmp(a, "object")) {
                o = &objects[numObjects++];
                memcpy(o, &defobj, sizeof(struct object));
                for(j = 0; j < o->numVerbs; j++) {
                    r = o->verbs[j];
                    o->verbs[j] = malloc(strlen(r));
                    strcpy(o->verbs[j], r);
                    r = o->verbNames[j];
                    o->verbNames[j] = malloc(strlen(r));
                    strcpy(o->verbNames[j], r);
                }
                r = 0;
                o->name = malloc(strlen(symbols[i+1])+1);
                strcpy(o->name, symbols[i+1]);
                o->title = malloc(strlen(symbols[i+2])+1);
                strcpy(o->title, symbols[i+2]);
                o->at = malloc(strlen(symbols[i+3])+1);
                strcpy(o->at, symbols[i+3]);
            }
            else if(!strcmp(a, "location")) {
                l = &locations[numLocations++];
                memcpy(l, &defloc, sizeof(struct location));
                l->name = malloc(strlen(symbols[i+1])+1);
                strcpy(l->name, symbols[i+1]);
                l->title = malloc(strlen(symbols[i+2])+1);
                strcpy(l->title, symbols[i+2]);
                l->description = malloc(strlen(symbols[i+3])+1);
                strcpy(l->description, symbols[i+3]);
            }
            else if(!strcmp(a, "items")) {
                sprintf(s, "%d", locationObjectNum(symbols[i+1]));
                r = s;
            }
            else if(!strcmp(a, "set-at")) {
                o = findObject(symbols[i+1]);
            if(o->at) free(o->at);
                o->at = malloc(strlen(symbols[i+2])+1);
                strcpy(o->at, symbols[i+2]);
            }
            else if(!strcmp(a, "get-at")) {
                o = findObject(symbols[i+1]);
                r = o->at;
            }
            else if(!strcmp(a, "set-exit")) {
                l = findLocation(symbols[i+1]);
                d = getDIndex(symbols[i+2]);
                if(l->exits[d]) free(l->exits[d]);
                if(strcmp(symbols[i+3], "nil")) {
                    l->exits[d] = malloc(strlen(symbols[i+3])+1);
                    strcpy(l->exits[d], symbols[i+3]);
                }
                else
                    l->exits[d] = 0;
            }
            else if(!strcmp(a, "get-exit")) {
                l = findLocation(symbols[i+1]);
                d = getDIndex(symbols[i+2]);
                r = l->exits[d];
                if(!r) r = "nil";
            }
            else if(!strcmp(a, "print")) {
                for(j = i+1; j < numSymbols; j++)
                    printf("%s", symbols[j]);
                printf("\n");
            }
            else if(!strcmp(a, "+")) {
                sprintf(s, "%d", atoi(symbols[i+1])+atoi(symbols[i+2]));
                r = s;
            }
            else if(!strcmp(a, "-")) {
                sprintf(s, "%d", atoi(symbols[i+1])-atoi(symbols[i+2]));
                r = s;
            }
            else if(!strcmp(a, "/")) {
                sprintf(s, "%d", atoi(symbols[i+1])/atoi(symbols[i+2]));
                r = s;
            }
            else if(!strcmp(a, "*")) {
                sprintf(s, "%d", atoi(symbols[i+1])*atoi(symbols[i+2]));
                r = s;
            }
            else if(strcmp(a, "begin"))
                fprintf(stderr, "undefined symbol %s\n", a);

            if(r) {
                a = malloc(strlen(r)+1);
                strcpy(a, r);
                r = a;
            }

            for(j = i-1; j < numSymbols; j++)
                free(symbols[j]);
            numSymbols = i-1;
            if(r)
                symbols[numSymbols++] = (char*)r;
        }
    }

    if(filename)
        fclose(fp);
}

void readWords() {
    char s[200];
    int i;

    for(i = 0; i < numWords; i++)
        free(words[i]);

    printf(">");
    numWords = 0;
    do {
        scanf("%s", s);
        for(i = 0; s[i]; i++)
            if(s[i] >= 'A' && s[i] <= 'Z')
                s[i] += 'a'-'A';
        words[numWords] = malloc(strlen(s)+1);
        strcpy(words[numWords++], s);
    } while(fgetc(stdin) != '\n');
    nextWordIndex = 0;
}

char *nextWord() {
    if(nextWordIndex >= numWords)
        return 0;
    return words[nextWordIndex++];
}

void listJoint(int i, int n) {
    if(i < n-2)
        printf(", ");
    else if(i < n-1)
        printf(" and ");
}

int listItems(const char *pre, char *lname) {
    int i, j, n;
    n = locationObjectNum(lname);
    if(n) {
        i = 0;
        printf(pre);
        for(j = 0; j < numObjects; j++)
            if(!strcmp(objects[j].at, lname)) {
                printf("%s", objects[j].title);
                listJoint(i, n);
                i++;
            }
        printf(".\n");
    }
    return n;
}

void takeInventory() {
    if(!listItems("You are carrying ", "inventory"))
        printf("You are not carrying anything.\n");
}

void listDirections(struct location *l, const char *pre, const char **strings) {
    int i, j, n;

    n = 0;
    for(i = 0; i < NUM_DIRS; i++)
        if(l->exits[i]) n++;
    if(n) {
        i = 0;
        printf(pre);
        for(j = 0; j < NUM_DIRS; j++)
            if(l->exits[j]) {
                printf(strings[j]);
                listJoint(i, n);
                i++;
            }
        printf(".\n");
    }
}

void describe() {
    struct location *l;
    l = findLocation(currentLocation);
    printf("%s\n", l->description);
    listItems("You see here ", currentLocation);
    listDirections(l, "You can exit ", dirStrings);
}

int main() {
    int i, j, n, r;
    char f;
    char *lastVerb="examine", *noun, *verb, *in;
    char comma = 0, unf = 0;
    struct location *l;
    struct object *o;
    const char *validVerbs[] = {
        "eat","read", "and", "examine", "take", "drop", "open", "close", "lock", "unlock", 0
    };

    eval("adv.sal", 0);

    describe();
    readWords();
    for(;;) {
outer:
        verb = nextWord();
        if(!verb) {
            readWords();
            continue;
        }

        if(!strcmp(verb, "look") || !strcmp(verb, "l")) {
            describe();
            readWords();
            continue;
        }

        if(!strcmp(verb, "inventory") || !strcmp(verb, "i")) {
            takeInventory();
            readWords();
            continue;
        }

        if(!strcmp(verb, "quit") || !strcmp(verb, "q")) {
            printf("Bye-bye!\n");
            break;
        }

        if(!strcmp(verb, "go") || !strcmp(verb, "move")) {
            verb = nextWord();
            if(!verb) {
                printf("Which direction?\n");
                continue;
            }
        }

        /* go direction */
        l = findLocation(currentLocation);
        for(i = 0; i < NUM_DIRS; i++)
            if(!strcmp(verb, dStrings[i]) || !strcmp(verb, dirStrings[i])) {
                if(l->exits[i]) {
                    currentLocation = l->exits[i];
                    l = findLocation(currentLocation);
                    printf("[%s]\n", l->title);
                }
                else
                    printf("You can't go that way.\n");
                readWords();
                goto outer;
            }

        /* verb-noun from here on */

        if(!strcmp(verb, "and")) {
            if(strcmp(lastVerb, ""))
                strcpy(verb, lastVerb);
        }
        else if(comma) {
            noun = verb;
            verb = lastVerb;
        }

        f = 0;
        for(i = 0; validVerbs[i] && !f; i++)
            if(!strcmp(validVerbs[i], verb))
                f = 1;
        if(!f && !unf) {
            printf("%s?\n", verb);
            continue;
        }
        if(f)
            unf = 0;

        if(!words[nextWordIndex] && !unf && !comma) {
            printf("%s what?\n", verb);
            unf = 1;
            lastVerb = verb;
            continue;
        }

        if(unf) {
            unf = 0;
            noun = verb;
            verb = lastVerb;
        }
        else if(!comma)
            noun = nextWord();
        comma = 0;

        if(noun[strlen(noun)-1] == ',') {
            noun[strlen(noun)-1] = 0;
            comma = 1;
        }

        if(!strcmp(verb, "take") && !strcmp(noun, "inventory")) {
            takeInventory();
            continue;
        }

        o = findObject(noun);
        if(!o) {
            printf("You know no such thing.\n");
            continue;
        }

        if(!strcmp(o->at, "inventory") && !strcmp(o->at, currentLocation)) {
            printf("You can't see that.");
            continue;
        }

        doVerb(o, verb);
        lastVerb = verb;
    }

    return 0;
}
