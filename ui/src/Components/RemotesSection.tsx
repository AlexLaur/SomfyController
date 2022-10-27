import { useState, useEffect } from "react";
import { Container, Grid, Table } from "@mantine/core";
import axios from "axios";

import RemoteRow from "./RemoteRow";

const RemotesSection = () => {
  const [remotes, setRemotes] = useState<any[]>([]);

  useEffect(() => {
    async function fetchRemotes() {
      try {
        const remotesQueryset = await axios.get("/remotes");
        setRemotes(remotesQueryset.data);
      } catch (error) {
        console.log(error);
      }
    }
    fetchRemotes();
  }, []);

  return (
    <section id="remotes-section">
      <Container size="xl">
        <Grid justify="space-around">
          <Grid.Col xs={12} sm={12} md={12} lg={12}>
            <Table>
              <thead>
                <tr>
                  <th>ID</th>
                  <th>Name</th>
                  <th>Enabled</th>
                  <th>Actions</th>
                  <th>Rolling code</th>
                  <th>Configuration</th>
                </tr>
              </thead>
              <tbody>
                {remotes?.map((remote) => (
                  <RemoteRow remote={remote} key={remote.id} />
                ))}
              </tbody>
            </Table>
          </Grid.Col>
        </Grid>
      </Container>
    </section>
  );
};

export default RemotesSection;
