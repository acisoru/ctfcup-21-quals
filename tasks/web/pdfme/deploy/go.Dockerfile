FROM golang:1.16 as builder
WORKDIR /app
COPY src .
RUN CGO_ENABLED=0 GOOS=linux go build -a -o srv .

FROM alpine:latest
RUN apk --no-cache add ca-certificates
WORKDIR /root/
COPY --from=builder /app/srv .
RUN mkdir static
COPY --from=builder /app/static/index.html static/index.html
COPY flag.txt /etc/flag
CMD "/root/srv"

