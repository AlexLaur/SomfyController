import { useState } from "react";
import { Grid, Modal, Text, Button, Group } from "@mantine/core";
import axios from "axios";

import RemoteSwitch from "./RemoteSwitch";
import { RemoteActionButton, RemoteConfigurationButton } from "./RemoteButtons";

interface Remote {
  id: number;
  name: string;
  enabled: boolean;
  rolling_code: number;
}

interface IRemoteRow {
  remote: Remote;
}

function RemoteRow(props: IRemoteRow) {
  const [enabled, setEnabled] = useState(props.remote.enabled);
  const [rollingCode, setRollingCode] = useState(props.remote.rolling_code);
  const [progModalOpened, setProgModalOpened] = useState(false);
  const [resetModalOpened, setResetModalOpened] = useState(false);

  const commandAction = (url: string) => {
    try {
      axios.get(url);
      setRollingCode(rollingCode + 1);
    } catch (error) {
      console.error(error);
    }
  };

  const handleEnabled = () => {
    if (enabled) {
      axios.get(`/blind?action=enable&remote_id=${props.remote.id}`);
    } else {
      axios.get(`/blind?action=disable&remote_id=${props.remote.id}`);
    }
    setEnabled(!enabled);
  };

  const handleStopClicked = () => {
    commandAction(`/blind?action=stop&remote_id=${props.remote.id}`);
  };

  const handleUpClicked = () => {
    commandAction(`/blind?action=up&remote_id=${props.remote.id}`);
  };

  const handleDownClicked = () => {
    commandAction(`/blind?action=down&remote_id=${props.remote.id}`);
  };

  const handleProgClicked = () => {
    setProgModalOpened(true);
  };

  const handleResetRollingCodeClicked = () => {
    setResetModalOpened(true);
  };

  return (
    <>
      <Modal
        opened={progModalOpened}
        onClose={() => setProgModalOpened(false)}
        title="Warning Prog Button."
        centered
      >
        <Text>The prog button allows you to link or unlink this remote with your blind.</Text>
        <Text>If the remote and the blind are paired, both will be unpaired.</Text>

        <Group mt="xl">
          <Button variant="outline" onClick={() => {
            setProgModalOpened(false);
            commandAction(`/blind?action=prog&remote_id=${props.remote.id}`);
          }}>
            Continue
          </Button>
          <Button onClick={() => {
            setProgModalOpened(false);
          }}>
            Cancel
          </Button>
        </Group>
      </Modal>
      <Modal
        opened={resetModalOpened}
        onClose={() => setResetModalOpened(false)}
        title="Warning Reset Button."
        centered
      >
        <Text>The reset button will reset the rolling code of the remote.</Text>
        <Text>If the remote and the blind are paired, this action will unpair both. You should use this command only for tests or after the unpaired trhougt the Prog button.</Text>

        <Group mt="xl">
          <Button variant="outline" onClick={() => {
            setResetModalOpened(false);
            axios.get(`/blind?action=reset&remote_id=${props.remote.id}`);
            setRollingCode(0);
          }}>
            Continue
          </Button>
          <Button onClick={() => {
            setResetModalOpened(false);
          }}>
            Cancel
          </Button>
        </Group>
      </Modal>
      <tr>
        <td>{props.remote.id}</td>
        <td>{props.remote.name}</td>
        <td>
          <RemoteSwitch enabled={enabled} onChange={handleEnabled} />
        </td>
        <td>
          <Grid gutter="sm" grow>
            <Grid.Col span={4}>
              <RemoteActionButton
                text="Up"
                enabled={enabled}
                onClicked={handleUpClicked}
              />
            </Grid.Col>
            <Grid.Col span={4}>
              <RemoteActionButton
                text="Down"
                enabled={enabled}
                onClicked={handleDownClicked}
              />
            </Grid.Col>
            <Grid.Col span={4}>
              <RemoteActionButton
                text="Stop"
                enabled={enabled}
                onClicked={handleStopClicked}
              />
            </Grid.Col>
          </Grid>
        </td>
        <td>{rollingCode}</td>
        <td>
          <Grid gutter="sm" grow>
            <Grid.Col span={6}>
              <RemoteConfigurationButton
                text="Prog"
                enabled={enabled}
                onClicked={handleProgClicked}
              />
            </Grid.Col>
            <Grid.Col span={6}>
              <RemoteConfigurationButton
                text="Reset"
                enabled={enabled}
                onClicked={handleResetRollingCodeClicked}
              />
            </Grid.Col>
          </Grid>
        </td>
      </tr>
    </>
  );
}

export default RemoteRow;
