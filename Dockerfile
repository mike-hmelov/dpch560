FROM rust:slim-bullseye AS builder

COPY src/ /app/src/
COPY Cargo.* /app/

WORKDIR /app

RUN apt update && apt install libsensors-dev -y

RUN cargo build --release

FROM scratch AS runtime

USER 1000

COPY --from=builder /app/target/release/ch560monitor /ch560monitor

ENTRYPOINT ["/ch560monitor"]