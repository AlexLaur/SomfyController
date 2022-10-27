import { Button } from "@mantine/core";

interface IRemoteButton {
  text: string;
  enabled: boolean;
  onClicked: Function;
}

export function RemoteActionButton(props: IRemoteButton) {
  const handleClicked = () => {
    props.onClicked();
  };

  if (props.enabled) {
    return (
      <Button onClick={handleClicked} fullWidth uppercase>
        {props.text}
      </Button>
    );
  }
  return (
    <Button onClick={handleClicked} fullWidth uppercase disabled>
      {props.text}
    </Button>
  );
}

export function RemoteConfigurationButton(props: IRemoteButton) {
  const handleClicked = () => {
    props.onClicked();
  };

  if (props.enabled) {
    return (
      <Button onClick={handleClicked} color="teal" fullWidth uppercase>
        {props.text}
      </Button>
    );
  }
  return (
    <Button onClick={handleClicked} color="teal" fullWidth uppercase disabled>
      {props.text}
    </Button>
  );
}
