# Makefile

# コンパイラとフラグの設定
CC = clang
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`

# ソースファイルと出力先の設定
SRC = src/main.c
OUTDIR = ./bin
TARGET = $(OUTDIR)/calculator

# デフォルトターゲット
all: $(OUTDIR) $(TARGET)

# バイナリディレクトリの作成
$(OUTDIR):
	mkdir -p $(OUTDIR)

# ターゲットのビルド
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# クリーンアップ
clean:
	rm -f $(TARGET)

.PHONY: all clean

