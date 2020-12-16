import React, { Component } from 'react';
import DistributedServer from './DistributedServer';
import { connect } from 'react-redux';
import { bindActionCreators } from 'redux';
import * as DistributedServersAction from './DistributedServersAction';
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
      const topicInfo = topic.split("/");
      const { distributedServerActions } = this.props;
      distributedServer['macAddress'] = topicInfo[topicInfo.length - 1];
      distributedServer['roomName'] = '';
      distributedServer['topicName'] = '';
      distributedServer['inputDeviceName'] = '';
      distributedServer['outputDeviceName'] = '';
      distributedServerActions.addDistributedServer(distributedServer);
    });
  }

  render(){
    const { distributedServers, handleOpen, handleClose } = this.props;
    return(
      <div style={{ display: 'flex', flexDirection: 'column' }}>
        <div style={{ display: 'flex', justifyContent: 'center' }}>
          <h1>Cômodos</h1>
        </div>
        <div
          style={{
            display: "grid",
            justifyItems: "center",
            gridGap: "10px",
            padding: "0px 16px",
            gridTemplateColumns: "repeat( auto-fill, minmax(350px, 1fr) )",
          }}
        >
          {distributedServers.map((server, index) => {
            return(
              <DistributedServer server={server} key={index} handleOpen={handleOpen} handleClose={handleClose}/>
            );
          })}
        </div>
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
  return({
    distributedServers: distributedServersReducer.distributedServers
  });
};

DistributedServerList = connect(mapStateToProps, mapDispatchToProps)(DistributedServerList)

export default DistributedServerList;
