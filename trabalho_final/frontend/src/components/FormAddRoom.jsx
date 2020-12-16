import React, { Component } from 'react';
import { ButtonGroup, Button, FormGroup, FormControl, TextField } from '@material-ui/core';
import { connect } from 'react-redux';
import { bindActionCreators } from 'redux';
import * as DistributedServersAction from './DistributedServersAction';
import * as mqtt from 'mqtt';

class FormAddRoom extends Component {
  constructor(props){
    super(props);
    this.client = mqtt.connect('ws://mqtt.eclipseprojects.io/mqtt');
  }

  handleChange = (event) => {
    const { distributedServerActions, server } = this.props;
    distributedServerActions.changeDistributedServerInfo({
      key: event.target.name,
      value: event.target.value,
      macAddress: server.macAddress,
    });
  }

  render() {
    const { server, handleClose, onSubmit } = this.props;
    return (
      <div>
        <FormGroup style={{ minWidth: '300px' }}>
          <FormControl style={{ margin: '6px 0px' }}>
            <TextField
              label="Cômodo"
              name="roomName"
              variant="outlined"
              value={server.roomName}
              onChange={this.handleChange}
            />
          </FormControl>
          <FormControl style={{ margin: '6px 0px' }}>
            <TextField
              label="Tópico"
              name="topicName"
              variant="outlined"
              value={server.topicName}
              onChange={this.handleChange}
            />
          </FormControl>
          <FormControl style={{ margin: '6px 0px' }}>
            <TextField
              label="Dispositivo de Entrada"
              variant="outlined"
              name="inputDeviceName"
              value={server.inputDeviceName}
              onChange={this.handleChange}
            />
          </FormControl>
          <FormControl style={{ margin: '6px 0px' }}>
            <TextField
              label="Dispositivo de Saída"
              variant="outlined"
              name="outputDeviceName"
              value={server.outputDeviceName}
              onChange={this.handleChange}
            />
          </FormControl>
          <ButtonGroup style={{ justifyContent: 'center', margin: '6px 0px' }}>
            <Button variant="outlined" onClick={(e) => { handleClose() }}>Fechar</Button>
            <Button variant="contained" color="primary" onClick={onSubmit}>Cadastrar</Button>
          </ButtonGroup>
        </FormGroup>
      </div>
    );
  }
}

function mapDispatchToProps (dispatch) {
  return {
    distributedServerActions: bindActionCreators(DistributedServersAction, dispatch)
  }
}

const mapStateToProps = (state, props) => {
  const { distributedServersReducer, clientReducer } = state.Reducers;
  const objIndex = distributedServersReducer.distributedServers.findIndex((obj => obj.macAddress == props.macAddress));
  return({
    server: distributedServersReducer.distributedServers[objIndex]
  });
};

FormAddRoom = connect(mapStateToProps, mapDispatchToProps)(FormAddRoom)

export default FormAddRoom;
