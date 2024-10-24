# Define variables
LEX=lex
YACC=yacc
CC=cc
TARGET=gocompiler
LEX_FILE=gocompiler.l
LEX_OUTPUT=lex.yy.c
YACC_FILE=gocompiler.y
YACC_OUTPUT=y.tab.c

# Default rule, executed when `make` is run
all: $(TARGET)

# Rule to build the lexer executable
$(TARGET): $(YACC_OUTPUT) $(LEX_OUTPUT)
	$(CC) $(YACC_OUTPUT) $(LEX_OUTPUT) -o $(TARGET)

# Rule to generate the y.tab.c file from yacc file
$(YACC_OUTPUT): $(YACC_FILE)
	$(YACC) -dv $(YACC_FILE)

# Rule to generate the lex.yy.c file from the Lex file
$(LEX_OUTPUT): $(LEX_FILE)
	$(LEX) $(LEX_FILE)

# Clean up generated files
clean:
	rm -f $(LEX_OUTPUT) $(TARGET)

# Run the lexer
run: $(TARGET)
	./$(TARGET)