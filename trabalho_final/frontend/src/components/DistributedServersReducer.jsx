const initialState = {
  distributedServers: []
};

const distributedServersReducer = (state = initialState, action) => {
  switch (action.type) {
    case 'ADD_DISTRIBUTED_SERVER':
      const alreadyRegistered = Boolean(state.distributedServers.find(server => server['macAddress'] === action.data.macAddress));
      if(alreadyRegistered || state.distributedServers.length > 5){
        return state;
      } else {
        return {
          ...state,
          distributedServers: [...state.distributedServers, ...[action.data]]
        };
      }

    case 'CHANGE_DISTRIBUTED_SERVER_INFO':
      const macAddress = action.data['macAddress'];
      const objIndex = state.distributedServers.findIndex((obj => obj.macAddress == macAddress));
      state.distributedServers[objIndex] = {
        ...state.distributedServers[objIndex],
        [action.data.key]: action.data.value
      }
      return {
        ...state,
        distributedServers: [...state.distributedServers]
      }

    default:
      return state;
  }
};

export default distributedServersReducer;
