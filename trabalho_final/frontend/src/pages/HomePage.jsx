import React, { Component } from 'react';
import * as mqtt from 'mqtt';

class HomePage extends Component {
  constructor(props){
    super(props);
    this.client = mqtt.connect('ws://mqtt.eclipseprojects.io/mqtt');
    this.client.on('connect', () => {
      this.client.subscribe('fse2020/150154003/central', (err) => {
      })
    })
    this.state = {
      msg: {}
    };
  }

  componentDidMount(){
    this.client.on('message', (topic, message) => {
      this.setState({ msg: JSON.parse(message.toString())['bme280'] });
    })
  }

  render() {
    const { msg } = this.state;
    return (
      <div style={{ display: 'flex', flexDirection: 'column' }}>
        <h2>Temperatura: {msg['temperature']}</h2>
        <h2>Umidade: {msg['humidity']}</h2>
      </div>
    );
  }
}

export default HomePage;
