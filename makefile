# Define variables
LEX=lex
CC=cc
TARGET=lexer
LEX_FILE=lexer.l
LEX_OUTPUT=lex.yy.c

# Default rule, executed when `make` is run
all: $(TARGET)

# Rule to build the lexer executable
$(TARGET): $(LEX_OUTPUT)
	$(CC) $(LEX_OUTPUT) -o $(TARGET)

# Rule to generate the lex.yy.c file from the Lex file
$(LEX_OUTPUT): $(LEX_FILE)
	$(LEX) $(LEX_FILE)

# Clean up generated files
clean:
	rm -f $(LEX_OUTPUT) $(TARGET)

# Run the lexer
run: $(TARGET)
	./$(TARGET)