import React, { Component } from 'react';
import {
  Card,
  Avatar,
  IconButton,
  CardHeader,
  CardContent,
  Grid,
  Switch
} from '@material-ui/core';
import AddIcon from '@material-ui/icons/Add';
import FormAddRoom from './FormAddRoom';
import * as mqtt from 'mqtt';

class DistributedServer extends Component {
  constructor(props){
    super(props);
    this.client = mqtt.connect('ws://mqtt.eclipseprojects.io/mqtt');
    this.state = {
      temperature: '',
      humidity: '',
      state: ''
    }
  }

  handleChange = (event) => {
    this.setState({ [event.target.name]: event.target.value });
  }

  onSubmit = () => {
    const { server } = this.props;
    this.client.publish(
      `fse2020/150154003/dispositivos/${server.macAddress}`,
      JSON.stringify({ room: server.topicName }),
      (err) => {
        const { handleClose } = this.props;
        handleClose();
        this.client.subscribe(
          `fse2020/150154003/${server.topicName}/+`,
          (err) => {
            this.client.on("message", (topic, message) => {
              const msg = JSON.parse(message.toString());
              if(msg.hasOwnProperty('temperature')){
                this.setState({ temperature: msg.temperature });
              } else if(msg.hasOwnProperty('humidity')){
                this.setState({ humidity: msg.humidity });
              } else if(msg.hasOwnProperty('state')){
                this.setState({ state: msg.state });
              }
            });
          }
        );
      }
    );
  }

  render(){
    const { temperature, humidity, state } = this.state;
    const { server, handleOpen, handleClose } = this.props;
    return(
      <Card>
        <CardHeader
          avatar={
            <Avatar aria-label="recipe">
              R
            </Avatar>
          }
          action={
            <IconButton onClick={
              () => {
                handleOpen(
                  <FormAddRoom
                    onSubmit={this.onSubmit}
                    handleClose={handleClose}
                    macAddress={server.macAddress}
                  />,
                  `Cadastrar ${server.macAddress}`
                )
              }
            }>
              <AddIcon />
            </IconButton>
          }
          title={server.roomName}
          subheader={server.macAddress}
        />
        <CardContent>
          <div style={{ display: 'flex', flexDirection: 'column' }}>
            <span>Temperatura: {temperature}°C</span>
            <span>Umidade: {humidity}%</span>
            <Grid component="label" container alignItems="center" spacing={1}>
              <span>{server.inputDeviceName}</span>
              <Grid item>Off</Grid>
              <Grid item>
                <Switch
                  checked={state}
                  name={server.inputDeviceName}
                  disabled={true}
                />
              </Grid>
              <Grid item>On</Grid>
            </Grid>
          </div>
        </CardContent>
      </Card>
    );
  }
}

export default DistributedServer;
