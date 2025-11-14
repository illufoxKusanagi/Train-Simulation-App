import { BasicChart } from "@/components/charts/basic-chart";
import PageLayout from "@/components/page-layout";
import { Button } from "@/components/ui/button";
import { Tabs, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { TabsContent } from "@radix-ui/react-tabs";

export default function OutputPage() {
  const labels: string = "Train Speeds";
  return (
    <PageLayout>
      <div className="flex flex-col gap-32 w-full">
        <Tabs>
          <TabsList className="grid w-full grid-cols-3 gap-1">
            <TabsTrigger value="train-speed" className="text-xs sm:text-sm">
              Train Speed
            </TabsTrigger>
            <TabsTrigger value="tractive-effort" className="text-xs sm:text-sm">
              Tractive Effort
            </TabsTrigger>
            <TabsTrigger value="power" className="text-xs sm:text-sm">
              Powers
            </TabsTrigger>
          </TabsList>
          <TabsContent value="train-speed">
            <div className="flex flex-col gap-4 w-full">
              <BasicChart title="Train Speed Chart" />
              <div className="flex flex-row gap-2 justify-end">
                <Button>Save Chart</Button>
                <Button>Save {labels} Data</Button>
                <Button>Save All Data</Button>
              </div>
            </div>
          </TabsContent>
          <TabsContent value="tractive-effort">
            <div className="flex flex-col gap-4 w-full">
              <BasicChart title="Tractive Effort Chart" />
              <div className="flex flex-row gap-2 justify-end">
                <Button>Save Chart</Button>
                <Button>Save {labels} Data</Button>
                <Button>Save All Data</Button>
              </div>
            </div>
          </TabsContent>
          <TabsContent value="power">
            <div className="flex flex-col gap-4 w-full">
              <BasicChart title="Power Chart" />
              <div className="flex flex-row gap-2 justify-end">
                <Button>Save Chart</Button>
                <Button>Save {labels} Data</Button>
                <Button>Save All Data</Button>
              </div>
            </div>
          </TabsContent>
        </Tabs>
      </div>
    </PageLayout>
  );
}
