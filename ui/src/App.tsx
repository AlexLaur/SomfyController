import {
  TypographyStylesProvider,
  MantineProvider,
  AppShell,
  Header,
  Text,
} from "@mantine/core";

import { theme } from "./theme";
import Content from "./Views/Content";
import Footer from "./Components/Footer";

function App() {
  return (
    <MantineProvider theme={theme} withGlobalStyles withNormalizeCSS>
      <TypographyStylesProvider>
        <AppShell
          padding="sm"
          header={
            <Header height={60} p="xs">
              <Text size="xl" weight={700} color="white" mb="10px">
                Somfy Controller
              </Text>
            </Header>
          }
          styles={(theme) => ({
            main: {
              backgroundColor:
                theme.colorScheme === "dark"
                  ? theme.colors.dark[8]
                  : theme.colors.gray[0],
            },
          })}
          footer={<Footer />}
        >
          <Content />
        </AppShell>
      </TypographyStylesProvider>
    </MantineProvider>
  );
}

export default App;
