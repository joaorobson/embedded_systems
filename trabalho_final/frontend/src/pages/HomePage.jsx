import React, { Component } from "react";
import * as mqtt from "mqtt";
import CustomizedSwitches from "./RaspSensors";

class HomePage extends Component {
  constructor(props) {
    super(props);
    this.client = mqtt.connect("ws://mqtt.eclipseprojects.io/mqtt");
    this.client.on("connect", () => {
      this.client.subscribe("fse2020/150154003/central", (err) => {});
    });
    this.state = {
      msg: {},
    };
  }

  componentDidMount() {
    this.client.on("message", (topic, message) => {
      this.setState({ msg: JSON.parse(message.toString())["bme280"] });
    });
  }

  render() {
    const { msg } = this.state;
    return (
      <div style={{ display: "flex", flexDirection: "column" }}>
        <div style={{ display: "flex", justifyContent: "center" }}>
          <h2>Sensores: Servidor central</h2>
        </div>
        <div
          style={{
            display: "flex",
            flexDirection: "row",
            justifyContent: "space-around",
            alignItems: "center",
          }}
        >
          <h2>Temperatura: {msg["temperature"]}</h2>
          <h2>Umidade: {msg["humidity"]}</h2>
        </div>
        <CustomizedSwitches client={this.client} />
      </div>
    );
  }
}

export default HomePage;
