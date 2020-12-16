import React, { Component } from 'react';
import DistributedServer from './DistributedServer';
import * as mqtt from 'mqtt';

class DistributedServerList extends Component {
  constructor(props){
    super(props);
    this.client = mqtt.connect('ws://mqtt.eclipseprojects.io/mqtt');
    this.client.on('connect', () => {
      this.client.subscribe('fse2020/150154003/dispositivos/+', (err) => {});
    });
    this.state = {
      distributedServers: []
    }
  }

  componentDidMount(){
    this.client.on('message', (topic, message) => {
      let distributedServer = {};
      const macAddress = topic.split("/");
      distributedServer['macAddress'] = macAddress[macAddress.length - 1];
      this.setState(prevState => ({
        distributedServers: [...prevState.distributedServers, distributedServer]
      }))
    })
  }

  render(){
    const { distributedServers } = this.state;
    const { handleOpen, handleClose } = this.props;
    return(
      <div>
        <h1>Comôdos</h1>
        {distributedServers.map((server, index) => {
          return(
            <DistributedServer server={server} key={index} handleOpen={handleOpen} handleClose={handleClose}/>
          );
        })}
      </div>
    );
  }
}

export default DistributedServerList;
