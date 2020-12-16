import React, { Component } from 'react';
import { ButtonGroup, Button, FormGroup, FormControl, TextField } from '@material-ui/core';

class FormAddRoom extends Component {
  onSubmit = (event) => {
  }

  render() {
    const {
      handleClose,
      room_name,
      topic_name,
      input_device_name,
      output_device_name,
      handleChange
    } = this.props;

    return (
      <div>
        <FormGroup style={{ minWidth: '300px' }}>
          <FormControl style={{ margin: '6px 0px' }}>
            <TextField
              label="Cômodo"
              name="room_name"
              variant="outlined"
              value={room_name}
              onChange={handleChange}
            />
          </FormControl>
          <FormControl style={{ margin: '6px 0px' }}>
            <TextField
              label="Tópico"
              name="topic_name"
              variant="outlined"
              value={topic_name}
              onChange={handleChange}
            />
          </FormControl>
          <FormControl style={{ margin: '6px 0px' }}>
            <TextField
              label="Dispositivo de Entrada"
              variant="outlined"
              name="input_device_name"
              value={input_device_name}
              onChange={handleChange}
            />
          </FormControl>
          <FormControl style={{ margin: '6px 0px' }}>
            <TextField
              label="Dispositivo de Saída"
              variant="outlined"
              name="output_device_name"
              value={output_device_name}
              onChange={handleChange}
            />
          </FormControl>
          <ButtonGroup style={{ justifyContent: 'center', margin: '6px 0px' }}>
            <Button variant="outlined" onClick={(e) => { handleClose() }}>Fechar</Button>
            <Button variant="contained" color="primary" onClick={() => {}}>Cadastrar</Button>
          </ButtonGroup>
        </FormGroup>
      </div>
    );
  }
}

export default FormAddRoom;
