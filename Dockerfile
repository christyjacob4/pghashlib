ARG PG_MAJOR=17
FROM postgres:$PG_MAJOR
ARG PG_MAJOR

COPY . /tmp/pg_hello_world
WORKDIR /tmp/pg_hello_world

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
            build-essential \
            postgresql-server-dev-$PG_MAJOR && \
    make clean && \
    make OPTFLAGS="" && \
    make install && \
    chmod -R 777 /tmp/pg_hello_world