OUTDIR		:= final_dir

CC          := gcc
CC_FLAGS    := -pedantic -Wall -g

LD			:= gcc
LD_LIBS		:= -lpthread -lncurses -lrt

RM			:= rm -rf
MKDIR 		:= mkdir -p

all: run_server

.PHONY: all

run_server: build
	./${OUTDIR}/server

run_client:
	./${OUTDIR}/client

run_bot:
	./${OUTDIR}/bot

run_beast:
	./${OUTDIR}/beast

build: build_server build_client build_bot build_beast

rebuild: clean build

build_server: .prepare prep_server
build_client: .prepare prep_client
build_bot: .prepare prep_bot
build_beast: .prepare prep_beast


prep_server: ${OUTDIR}/server.o ${OUTDIR}/multi_header.o 
	@ echo "Server consolidation ..."
	${LD} ${OUTDIR}/server.o ${OUTDIR}/multi_header.o -o ${OUTDIR}/server ${LD_LIBS}

prep_client: ${OUTDIR}/client.o ${OUTDIR}/client_helper.o 
	@ echo "Client consolidation ..."
	${LD} ${OUTDIR}/client.o ${OUTDIR}/client_helper.o -o ${OUTDIR}/client ${LD_LIBS}

prep_bot: ${OUTDIR}/bot.o ${OUTDIR}/bot_helper.o ${OUTDIR}/client_helper.o
	@ echo "Bot consolidation ..."
	${LD} ${OUTDIR}/bot.o ${OUTDIR}/bot_helper.o ${OUTDIR}/client_helper.o -o ${OUTDIR}/bot ${LD_LIBS}

prep_beast: ${OUTDIR}/beast.o ${OUTDIR}/beast_helper.o ${OUTDIR}/client_helper.o ${OUTDIR}/bot_helper.o
	@ echo "Beast consolidation ..."
	${LD} ${OUTDIR}/beast.o ${OUTDIR}/beast_helper.o ${OUTDIR}/client_helper.o ${OUTDIR}/bot_helper.o -o ${OUTDIR}/beast ${LD_LIBS}

.PHONY: run_server run_client run_bot run_beast
.PHONY: build rebuild build_server build_client build_bot build_beast
.PHONY: prep_server prep_client prep_bot prep_beast

${OUTDIR}/server.o: server.c multi_header.h
	@ echo "Building a file 'server.o' from 'server.c' ... "
	${CC} ${CC_FLAGS} -c server.c -o $@

${OUTDIR}/multi_header.o: multi_header.c multi_header.h
	@ echo "Building a file 'multi_header.o' from 'multi_header.c' ... "
	${CC} ${CC_FLAGS} -c multi_header.c -o $@

${OUTDIR}/client.o: client.c multi_header.h
	@ echo "Building a file 'client.o' from 'client.c' ... "
	${CC} ${CC_FLAGS} -c client.c -o $@

${OUTDIR}/client_helper.o: client_helper.c multi_header.h
	@ echo "Building a file 'client_helper.o' from 'client_helper.c' ... "
	${CC} ${CC_FLAGS} -c client_helper.c -o $@

${OUTDIR}/bot.o: bot.c multi_header.h
	@ echo "Building a file 'bot.o' from 'bot.c' ... "
	${CC} ${CC_FLAGS} -c bot.c -o ${OUTDIR}/bot.o

${OUTDIR}/bot_helper.o: bot_helper.c multi_header.h
	@ echo "Building a file 'bot_helper.o' from 'bot_helper.c' ... "
	${CC} ${CC_FLAGS} -c bot_helper.c -o $@

${OUTDIR}/beast.o: beast.c multi_header.h
	@ echo "Building a file 'beast.o' from 'beast.c' ... "
	${CC} ${CC_FLAGS} -c beast.c -o $@

${OUTDIR}/beast_helper.o: beast_helper.c multi_header.h
	@ echo "Building a file 'beast_helper.o' from 'beast_helper.c' ... "
	${CC} ${CC_FLAGS} -c beast_helper.c -o $@


.prepare: ${OUTDIR}

${OUTDIR}:
	${MKDIR} ${OUTDIR}

clean:
	${RM} ${OUTDIR}

.PHONY: prepare clean
