docker build -f DockerfileDev -t verifier-dev-env .
docker run -it --rm \
  -v "$PWD":/verifier \
  -w /verifier \
  verifier-dev-env
