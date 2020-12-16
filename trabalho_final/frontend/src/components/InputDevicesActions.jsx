export const actionUpdateInputDevices = (data) => ({
  type: 'UPDATE_DEVICES',
  data
})

export const updateInputDevices = (data, mqttClient) => {
  return dispatch => {
    dispatch(actionUpdateInputDevices({data, mqttClient}));
  }
}
