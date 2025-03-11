# A Simple Makefile to easily run examples/test source code.
# For only Header Libraries with my basic repository structure.
#
# Usage :
#		make sample
#		make clean

CC = gcc
CCFLAGS = -Wall -Wextra -std=c23 -Wformat -Wempty-body
CCWARNS = -Wunused-function -Wempty-body -Wunused-variable

# Comment/Uncomment to run examples/tests
FOLDER = ./examples

# Comment/Uncomment to run examples/tests
# FOLDER = ./tests

all : build run clean

% :
	@$(CC) $(CCFLAGS) $(FOLDER)/$@.c -I.  -o $@ 
	@./$@

build:
	$(CC) $(CCFLAGS) $(FOLDER)/sample.c  I.

run:
	@./a

clean:
	@rm -f ./*.o
	@rm -f ./*.exe


# ZZZZZZZZZZZZZZZZZZZ  EEEEEEEEEEEEEEEEEEEEEE  KKKKKKKKK    KKKKKKK  EEEEEEEEEEEEEEEEEEEEEE
# Z:::::::::::::::::Z  E::::::::::::::::::::E  K:::::::K    K:::::K  E::::::::::::::::::::E
# Z:::::::::::::::::Z  E::::::::::::::::::::E  K:::::::K    K:::::K  E::::::::::::::::::::E
# Z:::ZZZZZZZZ:::::Z   EE::::::EEEEEEEEE::::E  K:::::::K   K::::::K  EE::::::EEEEEEEEE::::E
# ZZZZZ     Z:::::Z      E:::::E       EEEEEE  KK::::::K  K:::::KKK    E:::::E       EEEEEE
#         Z:::::Z        E:::::E                K:::::K K:::::K        E:::::E
#        Z:::::Z         E::::::EEEEEEEEEE      K::::::K:::::K         E::::::EEEEEEEEEE
#       Z:::::Z          E:::::::::::::::E      K:::::::::::K          E:::::::::::::::E
#      Z:::::Z           E:::::::::::::::E      K:::::::::::K          E:::::::::::::::E
#     Z:::::Z            E::::::EEEEEEEEEE      K::::::K:::::K         E::::::EEEEEEEEEE
#    Z:::::Z             E:::::E                K:::::K K:::::K        E:::::E
# ZZZ:::::ZZZZZZZZZZZ    E:::::E       EEEEEE  KK::::::K  K:::::KKK    E:::::E       EEEEEE
# Z:::::::::::::::::Z  E::::::::::::::::::::E  K:::::::K   K::::::K  E::::::::::::::::::::E
# Z:::::::::::::::::Z  E::::::::::::::::::::E  K:::::::K    K:::::K  E::::::::::::::::::::E
# ZZZZZZZZZZZZZZZZZZZ  EEEEEEEEEEEEEEEEEEEEEE  KKKKKKKKK    KKKKKKK  EEEEEEEEEEEEEEEEEEEEEE
