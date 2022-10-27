import { Switch } from "@mantine/core";

interface IRemoteSwitch {
  enabled: boolean;
  onChange: Function;
}

function RemoteSwitch(props: IRemoteSwitch) {
  const handleChecked = () => {
    props.onChange();
  };

  return (
    <Switch
      onLabel="ON"
      offLabel="OFF"
      size="lg"
      checked={props.enabled}
      onChange={handleChecked}
    />
  );
}

export default RemoteSwitch;
