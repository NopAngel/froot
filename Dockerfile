FROM gcc:14.2.0 AS builder

WORKDIR /usr/src/froot

COPY . .

RUN make
FROM alpine:3.20

WORKDIR /app

COPY --from=builder ./froot /usr/local/bin/froot

RUN chmod +s /usr/local/bin/froot

CMD ["froot"]
