import {
  useMantineTheme,
  Container,
  Grid,
  Text,
  Group,
  Avatar,
  UnstyledButton,
  Anchor,
  Code,
  Badge
} from "@mantine/core";

const Footer = () => {
  const theme = useMantineTheme();

  return (
    <footer style={{ backgroundColor: theme.colors.gray[9] }}>
      <Container>
        <Grid justify="space-around">
          <Grid.Col xs={12} sm={8} md={8} lg={8}>
            <Text size="xl" weight={700} color="white" mb="10px">
              API
            </Text>
            <Text>
              <Badge color="green">GET</Badge> /blind?action=[up, down, stop, prog, reset, enable, disable]&remote_id=[id]
            </Text>
            <Text>
              <Badge color="green">GET</Badge> /remotes
            </Text>
          </Grid.Col>

          <Grid.Col xs={12} sm={4} md={4} lg={4}>
            <Code
              color="yellow"
              style={{
                display: "flex",
                flexDirection: "column",
                gap: 6,
                padding: 15,
              }}
            >
              Source code available on GitHub.
              <Anchor href="https://github.com/AlexLaur/SomfyController">
                <UnstyledButton>
                  <Group>
                    <Avatar size={40} color="blue">
                      SC
                    </Avatar>
                    <div>
                      <Text>SomfyController</Text>
                    </div>
                  </Group>
                </UnstyledButton>
              </Anchor>
            </Code>
          </Grid.Col>
        </Grid>
      </Container>
    </footer>
  );
};

export default Footer;
