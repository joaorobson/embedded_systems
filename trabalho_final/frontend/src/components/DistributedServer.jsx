import React, { Component } from 'react';
import {
  Card,
  Avatar,
  IconButton,
  Typography,
  CardHeader,
  CardContent,
} from '@material-ui/core';
import AddIcon from '@material-ui/icons/Add';
import FormAddRoom from './FormAddRoom';
import * as mqtt from 'mqtt';

class DistributedServer extends Component {
  constructor(props){
    super(props);
    this.client = mqtt.connect('ws://mqtt.eclipseprojects.io/mqtt');
    this.state = {
      room_name: null,
      topic_name: null,
      input_device_name: null,
      output_device_name: null,
    }
  }

  handleChange = (event) => {
    this.setState({ [event.target.name]: event.target.value });
  }

  render(){
    const { room_name } = this.state;
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
            <IconButton onClick={() => {handleOpen(<FormAddRoom handleClose={handleClose} handleChange={this.handleChange}/>, `Cadastrar ${server.macAddress}`)}}>
              <AddIcon />
            </IconButton>
          }
          title={room_name}
          subheader={server.macAddress}
        />
        <CardContent>
          <Typography variant="body2" color="textSecondary" component="p">
            This impressive paella is a perfect party dish and a fun meal to cook together with your
            guests. Add 1 cup of frozen peas along with the mussels, if you like.
          </Typography>
        </CardContent>
      </Card>
    );
  }
}

export default DistributedServer;
