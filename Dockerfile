FROM ubuntu:latest

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
        build-essential \
        cmake \
        ninja-build \
        git \
        python3 \
        python3-venv \
        python3-pip \
        openjdk-17-jre-headless \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /verifier
COPY . /verifier

RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --target Verifier

CMD ["./build/Verifier"]