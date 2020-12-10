import React, { Component } from 'react';
import * as mqtt from 'mqtt';

class HomePage extends Component {
  constructor(props){
    super(props);
    this.client = mqtt.connect({
      host: 'mqtt.eclipseprojects.io/mqtt',
      port: 80
    });
    this.client.on('connect', () => {
      this.client.subscribe('renatobolado', (err) => {
      })
    })
    this.state = {
      temperature: null
    };
  }

  componentDidMount(){
    this.client.on('message', (topic, message) => {
      this.setState({ temperature: parseFloat(message.toString()) });
    })
  }

  render() {
    const { temperature } = this.state;
    return (
      <div style={{ display: 'flex', flexDirection: 'column' }}>
        <h2>Temperatura: {temperature}</h2>
      </div>
    );
  }
}

export default HomePage;
