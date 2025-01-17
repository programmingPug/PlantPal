
<br/>
<div align="center">
<a href="https://github.com/programmingPug/plantpal">
<img src="https://imgur.com/BdczfrQ.png" alt="Logo" width="80" height="80">
</a>
<h3 align="center">PlantPal</h3>
<p align="center">
Because knowing when to water your plants is hard


  


</p>
</div>

## About The Project

![Prototype 1](https://imgur.com/TyOfD6j.png)

I love having plants around but I often forget to water them or how much to water them. So I initially started this project to just check if soil was dry and have a small piezo buzzer go off when a plant needed water. Well things escalated as I wanted to add features and now I'm at the point where it's a multi-system product with multiple moving parts. 

Where the project is now

At this point the project consists of:
- Probe nodes that record soil moisture data
- Reporter node that collects and sends the probe data to an API
- An API that serves to both the reporter node and a front-end client
- Front-end client that provides a web interface for viewing the collected plant moisture data.

Where the project is going

The end goal is to have the probe nodes collect soil moisture, soil temperature, relative humidity, air temperature and light spectrum values. With all this information I'm hoping to be able to optimism plant health and maybe even help figure out resource distribution for farming... but that's years down the road.
### Built With

This project is built on the following technologies:

- [NPM](https://www.npmjs.com/)
- [Node](https://nodejs.org/en)
- [Node](https://www.docker.com/)
- [Angular](https://angular.dev/)
- [.Net](https://dotnet.microsoft.com/en-us/download)
- [XAIO ESP32-C6](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/)
- [Eagle](https://www.autodesk.com/products/eagle/overview?term=1-YEAR&tab=subscription)
- [Arduino](https://www.arduino.cc/)

## Project Structures:

- plant-browser: Front-end client.
- SoilMoistureAPI: Backend API.
- Eagle: PCB project for soil nodes.
- batteryValidationTest: Arduino project for testing batteries.
- soilReporterNode: Arduino project for probe notes to report and forward data to API.
- soilProbeNode: Arduino project for gathering plant information.

## Requirement (Client and API)

- Node: v22.12.0
- NPM: v10.9.0
- Docker: v27.4.0

## Requirement (Hardware)
- Arduino IDE: v10
 - WiFiManager
 - Additional Boards: (https://espressif.github.io/arduino-esp32/package_esp32_index.json)
- XAIO ESP32C6
- Soil Sensor 2.0
- Miscellaneous electrical components

## Installing for Local Development:

### API:
You will need to modify a few things int he API project for local development.

1) program.cs
```js
options.ListenAnyIP(80) --> options.ListenAnyIP(8000)
```

2) appsettings.json
```js
"DefaultConnection": "Data Source=app/data/SoilMoisture.db" --> "DefaultConnection": "Data Source=SoilMoisture.db"
```

Then just click "http" to run the API.

## Install client dependencies:
From the client folder

Install dependencies
```sh
npm install
```

Run the Client:
```sh
ng s
```

## Building Images:
Building images for runtime distrobution

### API image:

Build docker image
```sh
docker build --no-cache -t plantpal-api:latest .
```

Tag image
```sh
docker image tag plantpal-api 192.168.1.193:5000/plantpal-api
```

Push to registry
```sh
docker push 192.168.1.193:5000/plantpal-api
```

### Client image:
Building images for runtime distrobution

Build docker image
```sh
docker build -t plantpal-client:latest .
```

Tag image
```sh
docker image tag plantpal-client 192.168.1.193:5000/plantpal-client
```

Push to registry
```sh
docker push 192.168.1.193:5000/plantpal-client
```

## Running Docker Images:

### Create Docker Network
Create new docker network for the containers to communicate
```sh
docker network create plantpal_network
```

### Run API Image

```sh
docker run -d --name plantpal_api --network plantpal_network -p 8000:80 plantpal_api:latest
```

### Run Client Image
```sh
docker run -d --name plantpal_client --network plantpal_network -p 3000:80 plantpal_client:latest
```

### Docker Compose:
optionally You can also use docker compose with the file at the root

```sh
docker-compose up -d --build
```



## Roadmap

- [X] Functional API
- [X] Functional Client
- [X] Functional reporter node
- [ ] Functional prob node
- [X] Unraid integration
- [X] Initial PCB design 
## Contributing

Currently this is closed to contributing till V1 is functional.
## License

Distributed under the MIT License. See [MIT License](https://opensource.org/licenses/MIT) for more information.
## Contact

Christopher Koch - ckoch@lazypug.net

Project Link: https://github.com/programmingPug/plantpal
