import { useEffect, useState, useRef } from "react";
import { Container, Grid, ScrollArea, Text, Paper, Title } from "@mantine/core";

const SerialSection = () => {

    const viewport = useRef<HTMLDivElement>(null);
    const [logs, setLogs] = useState([""]);

    useEffect(() => {
        async function openWebsocket() {
            const ws = new WebSocket(`ws://${window.location.hostname}/ws`);

            ws.onopen = (event) => {
                console.log("WS opened");
                setLogs([""]);
            };

            ws.onmessage = (event) => {
                setLogs(logs => [...logs, event.data]);
                scrollToBottom();
            }
        }
        openWebsocket();
    }, []);

    const scrollToBottom = () => viewport.current?.scrollTo({ top: viewport.current.scrollHeight, behavior: 'smooth' });

    return (
        <section id="serial-section">
            <Container size="xl">
                <Grid justify="space-around">
                    <Grid.Col xs={12} sm={12} md={12} lg={12}>
                        <Title order={3}>Logger</Title>
                        <Paper shadow="xs" p="md">
                            <ScrollArea style={{ height: 250 }} viewportRef={viewport}>
                                {logs.map(log => (<Text>{log}</Text>))}
                            </ScrollArea>
                        </Paper>
                    </Grid.Col>
                </Grid>
            </Container>
        </section>
    )
};

export default SerialSection;