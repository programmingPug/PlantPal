building and deploying:

docker build -t houseplant-client:latest .
docker image tag houseplant-client 192.168.1.193:5000/houseplant-client
docker push 192.168.1.193:5000/houseplant-client

docker build --no-cache -t houseplant-api:latest .
docker image tag houseplant-api 192.168.1.193:5000/houseplant-api
docker push 192.168.1.193:5000/houseplant-api