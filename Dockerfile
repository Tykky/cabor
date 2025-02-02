FROM frolvlad/alpine-glibc
WORKDIR /compiler
COPY build/release/bin/cabor /compiler/cabor
COPY build/release/lib/libuv.so.1.0.0 /compiler/libuv.so.1
COPY cabor_source.tar.gz /compiler/cabor_source.tar.gz
RUN chmod +x /compiler/cabor
ENTRYPOINT ["sh", "-c", "LD_LIBRARY_PATH=/app exec /app/cabor --server"]
