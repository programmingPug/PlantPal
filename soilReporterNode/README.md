
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

![Prototype 1](https://imgur.com/a/tMQBOKO)

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

This section should list any major frameworks/libraries used to bootstrap your project. Leave any add-ons/plugins for the acknowledgements section. Here are a few examples.

- [Angular](https://angular.dev/)
- [React](https://reactjs.org)
- [Vue](https://vuejs.org)
- [Angular](https://angular.io)
- [Svelte](https://svelte.dev)
- [Laravel](https://laravel.com)
- [Bootstrap](https://getbootstrap.com)
- [JQuery](https://jquery.com)
## Getting Started

This is an example of how you may give instructions on setting up your project locally.
To get a local copy up and running follow these simple example steps.
### Prerequisites

This is an example of how to list things you need to use the software and how to install them.

- npm
  ```sh
  npm install npm@latest -g
  ```
### Installation

_Below is an example of how you can instruct your audience on installing and setting up your app. This template doesn't rely on any external dependencies or services._

1. Get a free API Key at [https://example.com](https://example.com)
2. Clone the repo
   ```sh
   git clone https://github.com/your_username_/Project-Name.git
   ```
3. Install NPM packages
   ```sh
   npm install
   ```
4. Enter your API in `config.js`
   ```js
   const API_KEY = "ENTER YOUR API";
   ```
## Usage

Use this space to show useful examples of how a project can be used. Additional screenshots, code examples and demos work well in this space. You may also link to more resources.

_For more examples, please refer to the [Documentation](https://example.com)_
## Roadmap

- [x] Add Changelog
- [x] Add back to top links
- [ ] Add Additional Templates w/ Examples
- [ ] Add "components" document to easily copy & paste sections of the readme
- [ ] Multi-language Support
  - [ ] Chinese
  - [ ] Spanish

See the [open issues](https://github.com/ShaanCoding/ReadME-Generator/issues) for a full list of proposed features (and known issues).
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request
## License

Distributed under the MIT License. See [MIT License](https://opensource.org/licenses/MIT) for more information.
## Contact

Your Name - [@your_twitter](https://twitter.com/your_username) - email@example.com

Project Link: [https://github.com/your_username/repo_name](https://github.com/your_username/repo_name)
## Acknowledgments

Use this space to list resources you find helpful and would like to give credit to. I've included a few of my favorites to kick things off!


- [makeread.me](https://github.com/ShaanCoding/ReadME-Generator)
- [othneildrew](https://github.com/othneildrew/Best-README-Template)
