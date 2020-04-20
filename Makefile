CC := clang++
CFLAGS := -O2 -g -Wall -Werror -pedantic -std=c++14

.DEFAULT_GOAL := help

.PHONY: clean help

build: ## This builds executable
	$(CC) $(CFLAGS) ping.cc -o ping

run: ## This runs ping on a default ip
	./ping 192.168.86.14

clean: ## This removes the executable
	/bin/rm -f ping

help: ## This is the help dialog
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage:\n  make \033[36m<target>\033[0m\n\nTargets:\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-10s\033[0m %s\n", $$1, $$2 }' $(MAKEFILE_LIST)
