import React, { Component } from "react";
import Card from "@material-ui/core/Card";
import CardContent from "@material-ui/core/CardContent";
import Switch from "@material-ui/core/Switch";
import Grid from "@material-ui/core/Grid";

class CentralServerSensors extends Component {
  constructor(props) {
    super(props);

    this.client = props.client;

    this.state = {
      Sala: false,
      Cozinha: false,
      "Porta Cozinha": false,
      "Janela Cozinha": false,
      "Porta Sala": false,
      "Janela Sala": false,
      "Lâmpada Cozinha": false,
      "Lâmpada Sala": false,
    };
  }

  componentDidMount() {
    this.client.on("message", (topic, message) => {
      console.log(JSON.parse(message.toString()));
      this.setState((prevState) => ({
        ...prevState,
        ...JSON.parse(message.toString())["devices"],
      }));
    });
  }

  handleChange = (event) => {
    this.client.publish(
      "fse2020/150154003/frontend",
      JSON.stringify({ device_name: event.target.name })
    );
  };

  mountSensorsSwitches() {
    const sensorsList = [
      ["Sensor de presença 1", "Sala"],
      ["Sensor de presença 2", "Cozinha"],
      ["Sensor de abertura 1", "Porta Cozinha"],
      ["Sensor de abertura 2", "Janela Cozinha"],
      ["Sensor de abertura 3", "Porta Sala"],
      ["Sensor de abertura 4", "Janela Sala"],
      ["Lâmpada da Cozinha", "Lâmpada Cozinha"],
      ["Lâmpada da Sala", "Lâmpada Sala"],
    ];

    const outputSensors = ["Lâmpada da Cozinha", "Lâmpada da Sala"];

    const switches = sensorsList.map((sensor) => {
      return (
        <Card>
          <CardContent>
            <Grid component="label" container alignItems="center" spacing={1}>
              <div style={{ width: "100%" }}>
                <h3>
                  {sensor[0]} - {sensor[1]}
                </h3>
              </div>
              <Grid item>Off</Grid>
              <Grid item>
                <Switch
                  checked={this.state[sensor[1]]} // relevant state for your case
                  name={sensor[1]} // some value you need
                  onChange={this.handleChange}
                  disabled={outputSensors.includes(sensor[0]) ? false : true}
                />
              </Grid>
              <Grid item>On</Grid>
            </Grid>
          </CardContent>
        </Card>
      );
    });

    return switches;
  }

  render() {
    console.log(this.state);
    return (
      <div
        style={{
          display: "grid",
          justifyItems: "center",
          gridGap: "10px",
          padding: "0px 16px",
          gridTemplateColumns: "repeat( auto-fill, minmax(250px, 1fr) )",
        }}
      >
        {this.mountSensorsSwitches()}
      </div>
    );
  }
}

export default CentralServerSensors;
