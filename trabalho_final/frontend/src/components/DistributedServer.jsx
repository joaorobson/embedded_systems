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
import { connect } from 'react-redux';
import { bindActionCreators } from 'redux';
import * as InputDevicesActions from './InputDevicesActions';
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
      stateInput: false,
      stateOutput: 0
    }
  }

  handleChange = (event) => {
    this.setState({ [event.target.name]: event.target.value });
  }

  handleSelect = (event) => {
    const { server } = this.props;
    const { stateOutput } = this.state;
    this.client.publish(
      `fse2020/150154003/dispositivos/${server.macAddress}`,
      JSON.stringify({ led: 1 - stateOutput }),
      (err) => {
        this.setState({ stateOutput: 1 - stateOutput });
      }
    );
  };

  onSubmit = () => {
    const { server } = this.props;
    this.client.publish(
      `fse2020/150154003/dispositivos/${server.macAddress}`,
      JSON.stringify({ room: server.topicName }),
      (err) => {
        const { handleClose, inputDevicesActions } = this.props;
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
                this.setState({ stateInput: Boolean(msg.state) });
                inputDevicesActions.updateInputDevices({[`state_${server.macAddress}`]: Boolean(msg.state)}, this.client);
              }
            });
          }
        );
      }
    );
  }

  render(){
    const { server, handleOpen, handleClose } = this.props;
    const { temperature, humidity, stateInput, stateOutput } = this.state;
    return(
      <Card>
        <CardHeader
          avatar={
            <Avatar aria-label="recipe">
              {server.roomName[0]}
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
                  checked={stateInput}
                  disabled={true}
                  name={server.inputDeviceName}
                />
              </Grid>
              <Grid item>On</Grid>
            </Grid>
            <Grid component="label" container alignItems="center" spacing={1}>
              <span>{server.outputDeviceName}</span>
              <Grid item>Off</Grid>
              <Grid item>
                <Switch
                  checked={stateOutput}
                  onChange={this.handleSelect}
                  name={server.outputDeviceName}
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

function mapDispatchToProps (dispatch) {
  return {
    inputDevicesActions: bindActionCreators(InputDevicesActions, dispatch)
  }
}

DistributedServer = connect(null, mapDispatchToProps)(DistributedServer)

export default DistributedServer;
