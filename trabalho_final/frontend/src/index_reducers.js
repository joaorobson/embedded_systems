import { combineReducers } from 'redux';
import distributedServersReducer from './components/DistributedServersReducer';
import inputDevicesReducer from './components/InputDevicesReducer';

const Reducers = combineReducers({
  distributedServersReducer,
  inputDevicesReducer
});

export default Reducers;
