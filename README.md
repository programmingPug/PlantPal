
<br/>
<div align="center">
<a href="https://github.com/ShaanCoding/ReadME-Generator">
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

- [Angular](https://angular.dev/)
- [.Net](https://dotnet.microsoft.com/en-us/download)
- [XAIO ESP32-C6](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/)
- [Eagle](https://www.autodesk.com/products/eagle/overview?term=1-YEAR&tab=subscription)
- [Arduino](https://www.arduino.cc/)
### Installation


   ```sh
docker build -t plantpal-client:latest .
docker image tag plantpal-client 192.168.1.193:5000/plantpal-client
docker push 192.168.1.193:5000/plantpal-client
   ```

   ```sh
docker build --no-cache -t plantpal-api:latest .
docker image tag plantpal-api 192.168.1.193:5000/plantpal-api
docker push 192.168.1.193:5000/plantpal-api
   ```
## Roadmap

- [X] Functional API
- [ ] Functional Client
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

Project Link: https://github.com/programmingPug/houseplant
