export const actionAddDistributedServer = (data) => ({
  type: 'ADD_DISTRIBUTED_SERVER',
  data
})

export const actionChangeDistributedServerInfo = (data) => ({
  type: 'CHANGE_DISTRIBUTED_SERVER_INFO',
  data
})

export const addDistributedServer = (data) => {
  return dispatch => {
    dispatch(actionAddDistributedServer(data));
  }
}

export const changeDistributedServerInfo = (data) => {
  return dispatch => {
    dispatch(actionChangeDistributedServerInfo(data));
  }
}
