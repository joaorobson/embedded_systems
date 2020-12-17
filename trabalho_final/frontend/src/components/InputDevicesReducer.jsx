const initialState = {
  alarm: false,
  "Sala": false,
  "Cozinha": false,
  "Porta Cozinha": false,
  "Janela Cozinha": false,
  "Porta Sala": false,
  "Janela Sala": false,
  "Lâmpada Cozinha": false,
  "Lâmpada Sala": false
};

const inputDevicesReducer = (state = initialState, action) => {
  switch (action.type) {
    case 'UPDATE_DEVICES':
      let nextState = {
        ...state,
        ...action.data.data
      }
      let activateAlarm = false;
      for (const [key, value] of Object.entries(nextState)) {
        if(key !== 'alarm'){
          activateAlarm = value || activateAlarm;
        }
      }
      if(activateAlarm !== nextState.alarm){
        if(activateAlarm){
          action.data.mqttClient.publish(
            'fse2020/150154003/frontend',
            JSON.stringify({ alarm: activateAlarm })
          );
          nextState.alarm = activateAlarm;
        } else {
          action.data.mqttClient.publish(
            'fse2020/150154003/frontend',
            JSON.stringify({ alarm: activateAlarm })
          );
          nextState.alarm = activateAlarm;
        }
      }
      return nextState

    default:
      return state;
  }
};

export default inputDevicesReducer;
