import React, { Component } from 'react';
import Slide from '@material-ui/core/Slide';
import Dialog from '@material-ui/core/Dialog';
import DialogTitle from '@material-ui/core/DialogTitle';
import DialogContent from '@material-ui/core/DialogContent';
import DistributedServerList from '../components/DistributedServerList';
import CustomizedSwitches from "./RaspSensors";
import * as mqtt from 'mqtt';

const Transition = React.forwardRef(function Transition(props, ref) {
  return <Slide direction="up" ref={ref} {...props} />;
});

class HomePage extends Component {
  constructor(props) {
    super(props);
    this.client = mqtt.connect("ws://mqtt.eclipseprojects.io/mqtt");
    this.client.on("connect", () => {
      this.client.subscribe("fse2020/150154003/central", (err) => {});
    });
    this.state = {
      msg: {},
      modalTitle: '',
      component: null,
      isModalOpen: false,
    };
  }

  componentDidMount() {
    this.client.on("message", (topic, message) => {
      this.setState({ msg: JSON.parse(message.toString())["bme280"] });
    });
  }

  handleOpen = (component=undefined, modalTitle='') => {
    this.setState({
      isModalOpen: true,
      component: component,
      modalTitle: modalTitle,
    });
  }

  handleClose = () => {
    this.setState({
      isModalOpen: false,
      component: undefined,
      modalTitle: ''
    });
  }

  render() {
    const { msg, isModalOpen, component, modalTitle } = this.state;
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
          <h2>Temperatura: {msg["temperature"]}°C</h2>
          <h2>Umidade: {msg["humidity"]}%</h2>
        </div>
        <CustomizedSwitches client={this.client} />
        <DistributedServerList handleOpen={this.handleOpen} handleClose={this.handleClose} />
        <Dialog
          open={isModalOpen}
          onClose={this.handleClose}
          TransitionComponent={Transition}
        >
          <DialogTitle id="form-dialog-title">{modalTitle}</DialogTitle>
          <DialogContent>
            {component}
          </DialogContent>
        </Dialog>
      </div>
    );
  }
}

export default HomePage;
